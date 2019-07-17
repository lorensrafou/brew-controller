using System.Collections.Generic;
using System.Linq;
using CoreBrewServer.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

namespace CoreBrewServer.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class BrewDayController : Controller
    {
        private readonly BrewingContext _context;
        public BrewDayController(BrewingContext context){
            _context = context;
        }

        // GET: api/BrewDay
        [HttpGet]
        public IEnumerable<BrewDay> Get()
        {
            return _context.BrewDays
                .Include(b => b.BrewSchedule)
                .Include(bd => bd.MashTemperatures)
                .ToList();
        }

        // GET: api/BrewDay/1
        [HttpGet("{id}")]
        public IActionResult Get(int id)
        {
            BrewDay BrewDay = _context.BrewDays
                .Include(b => b.BrewSchedule)
                .Include(bd => bd.MashTemperatures)
                .FirstOrDefault(x => x.Id == id);

            return StatusCode(201, BrewDay);
        }

        // POST: api/BrewDay/
        [HttpPost]
        public IActionResult Post([FromBody] BrewDay bd)
        {
            BrewSchedule bs = _context.BrewSchedules
                .FirstOrDefault(x => x.Id == bd.BrewScheduleId);
            bd.BrewSchedule = bs;
            _context.BrewDays.Add(bd);
            _context.SaveChanges();
            return StatusCode(201, bd);
        }

        // PUT: api/BrewDay/1
        [HttpPut("{id}")]
        public IActionResult Put(int id, [FromBody] BrewDay bd)
        {
            _context.BrewDays.Update(bd);
            _context.SaveChanges();

            return StatusCode(201, bd);
        }

        // DELETE: api/BrewDay/1
        [HttpDelete("{id}")]
        public IActionResult Delete(int id)
        {
            BrewDay BrewDay = _context.BrewDays
                .Include(brewDay => brewDay.BrewSchedule)
                .Include(bd => bd.MashTemperatures)
                .FirstOrDefault(x => x.Id == id);
            _context.BrewDays.Remove(BrewDay);
            _context.SaveChanges();
            return StatusCode(201, BrewDay);
        }
    }
}